/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
import { memo, useEffect, useCallback, useMemo, useState, useRef } from 'react';
import { uniq, isEqual, sortBy, debounce, isEmpty } from 'lodash';
import {
  Filter,
  NativeFilterType,
  Divider,
  styled,
  t,
  css,
  useTheme,
} from '@superset-ui/core';
import { useDispatch } from 'react-redux';
import {
  Constants,
  Form,
  Icons,
  StyledModal,
} from '@superset-ui/core/components';
import { ErrorBoundary } from 'src/components';
import { testWithId } from 'src/utils/testUtils';
import { updateCascadeParentIds } from 'src/dashboard/actions/nativeFilters';
import useEffectEvent from 'src/hooks/useEffectEvent';
import { useFilterConfigMap, useFilterConfiguration } from '../state';
import FilterConfigurePane from './FilterConfigurePane';
import FiltersConfigForm, {
  FilterPanels,
} from './FiltersConfigForm/FiltersConfigForm';
import Footer from './Footer/Footer';
import { useOpenModal, useRemoveCurrentFilter } from './state';
import {
  FilterChangesType,
  FilterRemoval,
  NativeFiltersForm,
  SaveFilterChangesType,
} from './types';
import {
  createHandleSave,
  createHandleRemoveItem,
  generateFilterId,
  getFilterIds,
  validateForm,
  NATIVE_FILTER_DIVIDER_PREFIX,
  hasCircularDependency,
} from './utils';
import DividerConfigForm from './DividerConfigForm';

const MODAL_MARGIN = 16;
const MIN_WIDTH = 880;

const StyledModalWrapper = styled(StyledModal)<{ expanded: boolean }>`
  min-width: ${MIN_WIDTH}px;
  width: ${({ expanded }) => (expanded ? '100%' : MIN_WIDTH)} !important;

  @media (max-width: ${MIN_WIDTH + MODAL_MARGIN * 2}px) {
    width: 100% !important;
    min-width: auto;
  }

  .ant-modal-body {
    padding: 0px;
    overflow: auto;
  }

  ${({ expanded }) =>
    expanded &&
    css`
      height: 100%;

      .ant-modal-body {
        flex: 1 1 auto;
      }
      .ant-modal-content {
        height: 100%;
      }
    `}
`;

export const StyledModalBody = styled.div<{ expanded: boolean }>`
  display: flex;
  height: ${({ expanded }) => (expanded ? '100%' : '700px')};
  flex-direction: row;
  flex: 1;
  .filters-list {
    width: ${({ theme }) => theme.sizeUnit * 50}px;
    overflow: auto;
  }
`;

export const StyledForm = styled(Form)`
  width: 100%;
`;

export const StyledExpandButtonWrapper = styled.div`
  margin-left: ${({ theme }) => theme.sizeUnit * 4}px;
`;

export const FILTERS_CONFIG_MODAL_TEST_ID = 'filters-config-modal';
export const getFiltersConfigModalTestId = testWithId(
  FILTERS_CONFIG_MODAL_TEST_ID,
);

export interface FiltersConfigModalProps {
  isOpen: boolean;
  initialFilterId?: string;
  createNewOnOpen?: boolean;
  onSave: (filterChanges: SaveFilterChangesType) => Promise<void>;
  onCancel: () => void;
}
export const ALLOW_DEPENDENCIES = [
  'filter_range',
  'filter_select',
  'filter_time',
];

const DEFAULT_EMPTY_FILTERS: string[] = [];
const DEFAULT_REMOVED_FILTERS: Record<string, FilterRemoval> = {};
const DEFAULT_FORM_VALUES: NativeFiltersForm = {
  filters: {},
};
const DEFAULT_FILTER_CHANGES: FilterChangesType = {
  modified: [],
  deleted: [],
  reordered: [],
};

/**
 * This is the modal to configure all the dashboard-native filters.
 * Manages modal-level state, such as what filters are in the list,
 * and which filter is currently being edited.
 *
 * Calls the `save` callback with the new FilterConfiguration object
 * when the user saves the filters.
 */
function FiltersConfigModal({
  isOpen,
  initialFilterId,
  createNewOnOpen,
  onSave,
  onCancel,
}: FiltersConfigModalProps) {
  const dispatch = useDispatch();
  const theme = useTheme();

  const [form] = Form.useForm<NativeFiltersForm>();

  const configFormRef = useRef<any>();

  // the filter config from redux state, this does not change until modal is closed.
  const filterConfig = useFilterConfiguration();
  const filterConfigMap = useFilterConfigMap();

  // this state contains the changes that we'll be sent through the PATCH endpoint
  const [filterChanges, setFilterChanges] = useState<FilterChangesType>(
    DEFAULT_FILTER_CHANGES,
  );

  const resetFilterChanges = () => {
    setFilterChanges(DEFAULT_FILTER_CHANGES);
  };

  const handleModifyFilter = useCallback(
    (filterId: string) => {
      if (!filterChanges.modified.includes(filterId)) {
        setFilterChanges(prev => ({
          ...prev,
          modified: [...prev.modified, filterId],
        }));
      }
    },
    [filterChanges.modified],
  );

  // new filter ids belong to filters have been added during
  // this configuration session, and only exist in the form state until we submit.
  const [newFilterIds, setNewFilterIds] = useState<string[]>(
    DEFAULT_EMPTY_FILTERS,
  );

  // store ids of filters that have been removed with the time they were removed
  // so that we can disappear them after a few secs.
  // filters are still kept in state until form is submitted.
  const [removedFilters, setRemovedFilters] = useState<
    Record<string, FilterRemoval>
  >(DEFAULT_REMOVED_FILTERS);

  const [saveAlertVisible, setSaveAlertVisible] = useState<boolean>(false);

  // The full ordered set of ((original + new) - completely removed) filter ids
  // Use this as the canonical list of what filters are being configured!
  // This includes filter ids that are pending removal, so check for that.
  const filterIds = useMemo(
    () =>
      uniq([...getFilterIds(filterConfig), ...newFilterIds]).filter(
        id => !removedFilters[id] || removedFilters[id]?.isPending,
      ),
    [filterConfig, newFilterIds, removedFilters],
  );

  // open the first filter in the list to start
  const initialCurrentFilterId = initialFilterId ?? filterIds[0];
  const [currentFilterId, setCurrentFilterId] = useState(
    initialCurrentFilterId,
  );
  const [erroredFilters, setErroredFilters] = useState<string[]>(
    DEFAULT_EMPTY_FILTERS,
  );

  // the form values are managed by the antd form, but we copy them to here
  // so that we can display them (e.g. filter titles in the tab headers)
  const [formValues, setFormValues] =
    useState<NativeFiltersForm>(DEFAULT_FORM_VALUES);

  const unsavedFiltersIds = newFilterIds.filter(id => !removedFilters[id]);
  // brings back a filter that was previously removed ("Undo")
  const restoreFilter = useCallback(
    (id: string) => {
      const removal = removedFilters[id];
      // Clear the removal timeout if the filter is pending deletion
      if (removal?.isPending) clearTimeout(removal.timerId);

      setRemovedFilters(current => ({ ...current, [id]: null }));

      setFilterChanges(prev => ({
        ...prev,
        deleted: prev.deleted.filter(deletedId => deletedId !== id),
      }));
    },
    [removedFilters, setRemovedFilters],
  );
  const initialFilterOrder = useMemo(
    () => Object.keys(filterConfigMap),
    [filterConfigMap],
  );

  // State for tracking the re-ordering of filters
  const [orderedFilters, setOrderedFilters] =
    useState<string[]>(initialFilterOrder);

  // State for rendered filter to improve performance
  const [renderedFilters, setRenderedFilters] = useState<string[]>([
    initialCurrentFilterId,
  ]);

  const getActiveFilterPanelKey = (filterId: string) => [
    `${filterId}-${FilterPanels.configuration.key}`,
    `${filterId}-${FilterPanels.settings.key}`,
  ];

  const [activeFilterPanelKey, setActiveFilterPanelKey] = useState<
    string | string[]
  >(getActiveFilterPanelKey(initialCurrentFilterId));

  const handleTabChange = (filterId: string) => {
    setCurrentFilterId(filterId);
    setActiveFilterPanelKey(getActiveFilterPanelKey(filterId));
  };

  // generates a new filter id and appends it to the newFilterIds
  const addFilter = useCallback(
    (type: NativeFilterType) => {
      const newFilterId = generateFilterId(type);
      setNewFilterIds([...newFilterIds, newFilterId]);
      handleModifyFilter(newFilterId);
      setCurrentFilterId(newFilterId);
      setSaveAlertVisible(false);
      setOrderedFilters([...orderedFilters, newFilterId]);
      setActiveFilterPanelKey(getActiveFilterPanelKey(newFilterId));
    },
    [newFilterIds, handleModifyFilter, orderedFilters],
  );

  useOpenModal(isOpen, addFilter, createNewOnOpen);

  useRemoveCurrentFilter(
    removedFilters,
    currentFilterId,
    orderedFilters,
    setCurrentFilterId,
  );

  const handleRemoveItem = createHandleRemoveItem(
    setRemovedFilters,
    setOrderedFilters,
    setSaveAlertVisible,
    filterId => {
      setFilterChanges(prev => ({
        ...prev,
        deleted: [...prev.deleted, filterId],
      }));
    },
  );

  // After this, it should be as if the modal was just opened fresh.
  // Called when the modal is closed.
  const resetForm = (isSaving = false) => {
    setNewFilterIds(DEFAULT_EMPTY_FILTERS);
    setCurrentFilterId(initialCurrentFilterId);
    setRemovedFilters(DEFAULT_REMOVED_FILTERS);
    setSaveAlertVisible(false);
    setFormValues(DEFAULT_FORM_VALUES);
    resetFilterChanges();
    setErroredFilters(DEFAULT_EMPTY_FILTERS);
    if (filterIds.length > 0) {
      setActiveFilterPanelKey(getActiveFilterPanelKey(filterIds[0]));
    }
    if (!isSaving) {
      setOrderedFilters(initialFilterOrder);
    }
    setRenderedFilters([initialCurrentFilterId]);
    form.resetFields(['filters']);
    form.setFieldsValue({ changed: false });
  };

  const getFilterTitle = useCallback(
    (id: string) => {
      const formValue = formValues.filters[id];
      const config = filterConfigMap[id];
      return (
        (formValue && 'name' in formValue && formValue.name) ||
        (formValue && 'title' in formValue && formValue.title) ||
        (config && 'name' in config && config.name) ||
        (config && 'title' in config && config.title) ||
        t('[untitled]')
      );
    },
    [filterConfigMap, formValues.filters],
  );

  const canBeUsedAsDependency = useCallback(
    (filterId: string) => {
      if (removedFilters[filterId]) {
        return false;
      }
      const component =
        form.getFieldValue('filters')?.[filterId] || filterConfigMap[filterId];
      return (
        component &&
        'filterType' in component &&
        ALLOW_DEPENDENCIES.includes(component.filterType)
      );
    },
    [filterConfigMap, form, removedFilters],
  );

  const getAvailableFilters = useCallback(
    (filterId: string) =>
      filterIds
        .filter(id => id !== filterId)
        .filter(id => canBeUsedAsDependency(id))
        .map(id => ({
          label: getFilterTitle(id),
          value: id,
          type: filterConfigMap[id]?.filterType,
        })),
    [canBeUsedAsDependency, filterConfigMap, filterIds, getFilterTitle],
  );

  /**
   * Manages dependencies of filters associated with a deleted filter.
   *
   * @param values the native filters form
   * @returns the updated filterConfigMap
   */
  const cleanDeletedParents = (values: NativeFiltersForm | null) => {
    const modifiedParentFilters = new Set<string>();
    const updatedFilterConfigMap = Object.keys(filterConfigMap).reduce(
      (acc, key) => {
        const filter = filterConfigMap[key];
        const cascadeParentIds = filter.cascadeParentIds?.filter(id =>
          canBeUsedAsDependency(id),
        );

        if (
          cascadeParentIds &&
          !isEqual(cascadeParentIds, filter.cascadeParentIds)
        ) {
          dispatch(updateCascadeParentIds(key, cascadeParentIds));
          modifiedParentFilters.add(key);
        }

        return {
          ...acc,
          [key]: {
            ...filter,
            cascadeParentIds,
          },
        };
      },
      {},
    );

    const filters = values?.filters;
    if (filters) {
      Object.keys(filters).forEach(key => {
        const filter = filters[key];

        if (!('dependencies' in filter)) {
          return;
        }

        const originalDependencies = filter.dependencies || [];
        const cleanedDependencies = originalDependencies.filter(id =>
          canBeUsedAsDependency(id),
        );

        if (!isEqual(cleanedDependencies, originalDependencies)) {
          filter.dependencies = cleanedDependencies;
          modifiedParentFilters.add(key);
        }
      });
    }

    return [updatedFilterConfigMap, modifiedParentFilters];
  };

  const handleErroredFilters = useCallback(() => {
    // managing left pane errored filters indicators
    const formValidationFields = form.getFieldsError();
    const erroredFiltersIds: string[] = [];

    formValidationFields.forEach(field => {
      const filterId = field.name[1] as string;
      if (field.errors.length > 0 && !erroredFiltersIds.includes(filterId)) {
        erroredFiltersIds.push(filterId);
      }
    });

    // no form validation issues found, resets errored filters
    if (!erroredFiltersIds.length && erroredFilters.length > 0) {
      setErroredFilters(DEFAULT_EMPTY_FILTERS);
      return;
    }
    // form validation issues found, sets errored filters
    if (
      erroredFiltersIds.length > 0 &&
      !isEqual(sortBy(erroredFilters), sortBy(erroredFiltersIds))
    ) {
      setErroredFilters(erroredFiltersIds);
    }
  }, [form, erroredFilters]);

  const handleSave = async () => {
    const values: NativeFiltersForm | null = await validateForm(
      form,
      currentFilterId,
      setCurrentFilterId,
    );

    handleErroredFilters();

    if (values) {
      const [updatedFilterConfigMap, modifiedParentFilters] =
        cleanDeletedParents(values);

      const allModified = [
        ...new Set([
          ...(modifiedParentFilters as Set<string>),
          ...filterChanges.modified,
        ]),
      ];

      const actualChanges = {
        ...filterChanges,
        modified:
          allModified.length && filterChanges.deleted.length
            ? allModified.filter(id => !filterChanges.deleted.includes(id))
            : allModified,
        reordered:
          filterChanges.reordered.length &&
          !isEqual(filterChanges.reordered, initialFilterOrder)
            ? filterChanges.reordered
            : [],
      };

      createHandleSave(onSave, actualChanges, values, updatedFilterConfigMap)();
      resetForm(true);
      resetFilterChanges();
    } else {
      configFormRef.current?.changeTab?.('configuration');
    }
  };

  const handleConfirmCancel = () => {
    resetForm();
    onCancel();
  };

  const handleCancel = () => {
    const changed = form.getFieldValue('changed');
    const didChangeOrder =
      orderedFilters.length !== initialFilterOrder.length ||
      orderedFilters.some((val, index) => val !== initialFilterOrder[index]);
    if (
      unsavedFiltersIds.length > 0 ||
      form.isFieldsTouched() ||
      changed ||
      didChangeOrder ||
      Object.values(removedFilters).some(f => f?.isPending)
    ) {
      setSaveAlertVisible(true);
    } else {
      handleConfirmCancel();
    }
  };
  const handleRearrange = (dragIndex: number, targetIndex: number) => {
    const newOrderedFilter = [...orderedFilters];
    const removed = newOrderedFilter.splice(dragIndex, 1)[0];
    newOrderedFilter.splice(targetIndex, 0, removed);
    setOrderedFilters(newOrderedFilter);
    setFilterChanges(prev => ({
      ...prev,
      reordered: newOrderedFilter,
    }));
  };

  const buildDependencyMap = useCallback(() => {
    const dependencyMap = new Map<string, string[]>();
    const filters = form.getFieldValue('filters');
    if (filters) {
      Object.keys(filters).forEach(key => {
        const formItem = filters[key];
        const configItem = filterConfigMap[key];
        let array: string[] = [];
        if (formItem && 'dependencies' in formItem) {
          array = [...formItem.dependencies];
        } else if (configItem?.cascadeParentIds) {
          array = [...configItem.cascadeParentIds];
        }
        dependencyMap.set(key, array);
      });
    }
    return dependencyMap;
  }, [filterConfigMap, form]);

  const validateDependencies = useCallback(() => {
    const dependencyMap = buildDependencyMap();
    filterIds
      .filter(id => !removedFilters[id])
      .forEach(filterId => {
        const result = hasCircularDependency(dependencyMap, filterId);
        const field = {
          name: ['filters', filterId, 'dependencies'] as [
            'filters',
            string,
            'dependencies',
          ],
          errors: result ? [t('Cyclic dependency detected')] : [],
        };
        form.setFields([field]);
      });
    handleErroredFilters();
  }, [
    buildDependencyMap,
    filterIds,
    form,
    handleErroredFilters,
    removedFilters,
  ]);

  const getDependencySuggestion = useCallback(
    (filterId: string) => {
      const dependencyMap = buildDependencyMap();
      const possibleDependencies = orderedFilters.filter(
        key => key !== filterId && canBeUsedAsDependency(key),
      );
      const found = possibleDependencies.find(filter => {
        const dependencies = dependencyMap.get(filterId) || [];
        dependencies.push(filter);
        if (hasCircularDependency(dependencyMap, filterId)) {
          dependencies.pop();
          return false;
        }
        return true;
      });
      return found || possibleDependencies[0];
    },
    [buildDependencyMap, canBeUsedAsDependency, orderedFilters],
  );

  const [expanded, setExpanded] = useState(false);
  const toggleExpand = useEffectEvent(() => {
    setExpanded(!expanded);
  });
  const ToggleIcon = expanded
    ? Icons.FullscreenExitOutlined
    : Icons.FullscreenOutlined;

  const handleValuesChange = useMemo(
    () =>
      debounce((changes: any, values: NativeFiltersForm) => {
        const didChangeFilterName =
          changes.filters &&
          Object.values(changes.filters).some(
            (filter: any) => filter.name && filter.name !== null,
          );
        const didChangeSectionTitle =
          changes.filters &&
          Object.values(changes.filters).some(
            (filter: any) => filter.title && filter.title !== null,
          );
        if (didChangeFilterName || didChangeSectionTitle) {
          // we only need to set this if a name/title changed
          setFormValues(values);
        }
        setSaveAlertVisible(false);
        handleErroredFilters();
      }, Constants.SLOW_DEBOUNCE),
    [handleErroredFilters],
  );

  useEffect(() => {
    if (!isEmpty(removedFilters)) {
      setErroredFilters(prevErroredFilters =>
        prevErroredFilters.filter(f => !removedFilters[f]),
      );
    }
  }, [removedFilters]);

  useEffect(() => {
    if (!renderedFilters.includes(currentFilterId)) {
      setRenderedFilters([...renderedFilters, currentFilterId]);
    }
  }, [currentFilterId]);

  const handleActiveFilterPanelChange = useCallback(
    key => setActiveFilterPanelKey(key),
    [setActiveFilterPanelKey],
  );

  const formList = useMemo(
    () =>
      orderedFilters.map(id => {
        if (!renderedFilters.includes(id)) return null;
        const isDivider = id.startsWith(NATIVE_FILTER_DIVIDER_PREFIX);
        const isActive = currentFilterId === id;
        return (
          <div
            key={id}
            style={{
              height: '100%',
              overflowY: 'auto',
              display: isActive ? '' : 'none',
            }}
          >
            {isDivider ? (
              <DividerConfigForm
                componentId={id}
                divider={filterConfigMap[id] as Divider}
              />
            ) : (
              <FiltersConfigForm
                expanded={expanded}
                ref={configFormRef}
                form={form}
                filterId={id}
                filterToEdit={filterConfigMap[id] as Filter}
                removedFilters={removedFilters}
                restoreFilter={restoreFilter}
                getAvailableFilters={getAvailableFilters}
                key={id}
                activeFilterPanelKeys={activeFilterPanelKey}
                handleActiveFilterPanelChange={handleActiveFilterPanelChange}
                isActive={isActive}
                setErroredFilters={setErroredFilters}
                validateDependencies={validateDependencies}
                getDependencySuggestion={getDependencySuggestion}
                onModifyFilter={handleModifyFilter}
              />
            )}
          </div>
        );
      }),
    [
      orderedFilters,
      renderedFilters,
      currentFilterId,
      filterConfigMap,
      expanded,
      form,
      removedFilters,
      restoreFilter,
      getAvailableFilters,
      activeFilterPanelKey,
      handleActiveFilterPanelChange,
      validateDependencies,
      getDependencySuggestion,
      handleModifyFilter,
    ],
  );

  useEffect(() => {
    resetFilterChanges();
  }, []);

  return (
    <StyledModalWrapper
      open={isOpen}
      maskClosable={false}
      title={t('Add and edit filters')}
      expanded={expanded}
      destroyOnClose
      onCancel={handleCancel}
      onOk={handleSave}
      centered
      data-test="filter-modal"
      footer={
        <div
          css={css`
            display: flex;
            justify-content: flex-end;
            align-items: flex-end;
          `}
        >
          <Footer
            onDismiss={() => setSaveAlertVisible(false)}
            onCancel={handleCancel}
            handleSave={handleSave}
            canSave={!erroredFilters.length}
            saveAlertVisible={saveAlertVisible}
            onConfirmCancel={handleConfirmCancel}
          />
          <StyledExpandButtonWrapper>
            <ToggleIcon
              iconSize="l"
              iconColor={theme.colors.grayscale.dark2}
              onClick={toggleExpand}
            />
          </StyledExpandButtonWrapper>
        </div>
      }
    >
      <ErrorBoundary>
        <StyledModalBody expanded={expanded}>
          <StyledForm
            form={form}
            onValuesChange={handleValuesChange}
            layout="vertical"
          >
            <FilterConfigurePane
              erroredFilters={erroredFilters}
              onRemove={handleRemoveItem}
              onAdd={addFilter}
              onChange={handleTabChange}
              getFilterTitle={getFilterTitle}
              currentFilterId={currentFilterId}
              removedFilters={removedFilters}
              restoreFilter={restoreFilter}
              onRearrange={handleRearrange}
              filters={orderedFilters}
            >
              {formList}
            </FilterConfigurePane>
          </StyledForm>
        </StyledModalBody>
      </ErrorBoundary>
    </StyledModalWrapper>
  );
}

export default memo(FiltersConfigModal);
